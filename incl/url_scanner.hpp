#ifndef INCL_URL_SCANNER_HPP_
#define INCL_URL_SCANNER_HPP_

#include <QAbstractTableModel>
#include <QByteArray>
#include <QHash>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QThreadStorage>
#include <QUrl>
#include <QVector>

class UrlScanner : public QAbstractTableModel {
    Q_OBJECT

    Q_PROPERTY(QUrl start_url MEMBER m_start_url)
    Q_PROPERTY(int max_thread_count MEMBER m_max_thread_count)
    Q_PROPERTY(QString search_text MEMBER m_search_text)
    Q_PROPERTY(int max_urls_count MEMBER m_max_urls_count)

  public:
    explicit UrlScanner(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void stop();

  private:
    enum class ScanStatus { Ready, Scan, Download, Downloaded, Found, NotFound, Error };
    QString ScanStatusToString(const ScanStatus& scan_status);

    void Scan(const QUrl& url);
    void MultithreadedScan();

    void FindUrls(const QString& data);

    void SetUrlStatus(const QUrl& url, const ScanStatus& scan_status, const QString& error_string = "");

    QUrl m_start_url;
    int m_max_thread_count;
    QString m_search_text;
    int m_max_urls_count;

    bool m_pause;
    bool m_stop;

    static QThreadStorage<QNetworkAccessManager> m_access_manager;

    QMutex m_mutex;

    QMap<QUrl, ScanStatus> m_urls;

    QVector<QUrl> m_scanned_urls;
    QVector<QString> m_scanned_urls_statuses;

    const QRegularExpression m_url_reg_exp{
        "https?://[a-z0-9\\-\\.\\_\\~\\:\\/\\?\\#\\[\\]\\@\\!\\$\\&\\(\\)\\*\\+\\,\\;\\%\\=]+",
        QRegularExpression::CaseInsensitiveOption};
};

#endif  // INCL_URL_SCANNER_HPP_
