#include "incl/url_scanner.hpp"

#include <QApplication>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

QThreadStorage<QNetworkAccessManager> UrlScanner::m_access_manager;

UrlScanner::UrlScanner(QObject* parent) : QAbstractTableModel(parent)
{
}

int UrlScanner::rowCount(const QModelIndex&) const
{
    return m_scanned_urls.size();
}

int UrlScanner::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant UrlScanner::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return m_scanned_urls[index.row()];
            case 1:
                return m_scanned_urls_statuses[index.row()];
            default:
                return QVariant{};
        }
    }
    return QVariant{};
}

QHash<int, QByteArray> UrlScanner::roleNames() const
{
    return {{Qt::DisplayRole, "display"}};
}

void UrlScanner::start()
{
    if (m_max_thread_count > QThread::idealThreadCount() - 1) {
        m_max_thread_count = QThread::idealThreadCount() - 1;
    }
    QThreadPool::globalInstance()->setMaxThreadCount(m_max_thread_count + 1);
    m_scanned_urls.reserve(m_max_urls_count);
    m_scanned_urls_statuses.reserve(m_max_urls_count);
    beginResetModel();
    m_scanned_urls.push_back(m_start_url);
    m_scanned_urls_statuses.push_back(ScanStatusToString(ScanStatus::Ready));
    endResetModel();
    SetUrlStatus(m_start_url, ScanStatus::Scan);
    Scan(m_start_url);
    m_pause = false;
    m_stop = false;
    QtConcurrent::run(this, &UrlScanner::MultithreadedScan);
}

void UrlScanner::pause()
{
    m_pause = true;
}

void UrlScanner::resume()
{
    m_pause = false;
}

void UrlScanner::stop()
{
    m_stop = true;
}

void UrlScanner::Scan(const QUrl& url)
{
    QEventLoop event_loop;
    connect(&m_access_manager.localData(), &QNetworkAccessManager::finished, &event_loop,
            [this, &url, &event_loop](QNetworkReply* reply) {
                if (reply->error() == QNetworkReply::NetworkError::NoError) {
                    SetUrlStatus(url, ScanStatus::Downloaded);
                    QString downloaded_data(reply->readAll());
                    SetUrlStatus(url,
                                 downloaded_data.contains(m_search_text) ? ScanStatus::Found : ScanStatus::NotFound);
                    FindUrls(downloaded_data);
                } else {
                    SetUrlStatus(url, ScanStatus::Error, reply->errorString());
                }

                reply->deleteLater();

                event_loop.exit();
            },
            Qt::ConnectionType::DirectConnection);
    QNetworkRequest request(url);
    m_access_manager.localData().get(request);
    SetUrlStatus(url, ScanStatus::Download);
    event_loop.exec();
}

void UrlScanner::MultithreadedScan()
{
    while (!m_pause && !m_stop && m_scanned_urls.size() < m_max_urls_count && m_scanned_urls.size() < m_urls.size()) {
        if (QThreadPool::globalInstance()->activeThreadCount() < QThreadPool::globalInstance()->maxThreadCount()) {
            QUrl url = m_urls.key(ScanStatus::Ready);
            m_urls[url] = ScanStatus::Scan;
            beginResetModel();
            m_scanned_urls.push_back(url);
            m_scanned_urls_statuses.push_back(ScanStatusToString(ScanStatus::Scan));
            endResetModel();
            QtConcurrent::run(this, &UrlScanner::Scan, url);
        }
    }
    if (m_pause) {
        while (m_pause) {
        }
        MultithreadedScan();
    }
}

void UrlScanner::FindUrls(const QString& data)
{
    QRegularExpressionMatchIterator url_reg_exp_match_it = m_url_reg_exp.globalMatch(data);
    m_mutex.lock();
    while (url_reg_exp_match_it.hasNext()) {
        QRegularExpressionMatch url_reg_exp_match = url_reg_exp_match_it.next();
        QUrl url = url_reg_exp_match.captured(0);
        if (!m_urls.contains(url) && url.isValid()) {
            m_urls.insert(url, ScanStatus::Ready);
        }
    }
    m_mutex.unlock();
}

void UrlScanner::SetUrlStatus(const QUrl& url, const ScanStatus& scan_status, const QString& error_string)
{
    m_mutex.lock();
    m_urls[url] = scan_status;
    int row = m_scanned_urls.indexOf(url);
    m_scanned_urls_statuses[row] = error_string.isEmpty() ? ScanStatusToString(scan_status) : "Error: " + error_string;
    emit dataChanged(index(row, 1), index(row, 1));
    m_mutex.unlock();
}

QString UrlScanner::ScanStatusToString(const ScanStatus& scan_status)
{
    switch (scan_status) {
        case ScanStatus::Ready:
            return "Ready to scan";
        case ScanStatus::Scan:
            return "Scan";
        case ScanStatus::Download:
            return "Start download";
        case ScanStatus::Downloaded:
            return "Downloaded";
        case ScanStatus::Found:
            return "Text found";
        case ScanStatus::NotFound:
            return "Text not found";
        case ScanStatus::Error:
            return "Error";
        default:
            return "Undefined";
    }
}
