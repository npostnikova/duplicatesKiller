#ifndef DUPLICATESSEARCHER_H
#define DUPLICATESSEARCHER_H

#include <QString>
#include <QDir>
#include <QMap>
#include <cstdlib>
#include <unordered_map>
#include <map>
#include <vector>
#include "mytreeview.h"
#include "mylistview.h"
#include <mutex>
#include <cstdlib>


struct DuplicatesSearcher : QObject {
    Q_OBJECT
public:

    DuplicatesSearcher(QString const& dir, QWidget * duplicatesReceiver, QWidget * errorsReceiver);

    ~DuplicatesSearcher();

    bool isCanceled();
    
    static void checkFilesEquality(QDir directory, DuplicatesSearcher* searcher);

    void sendError(Message const& error);
signals:
    void setProgress(int);

    void updateProgress(int);

    void sendMessage(QString);

public slots:
    void interrupt() {
        stopped = true;
    }

private:
    std::vector<QByteArray> getSmallSizeContent(std::vector<QString>& names);

    static void runBlocksInThread(DuplicatesSearcher * searcher, std::vector<std::vector<QString>> names);

    void divideSmallFilesOnThreads(std::vector<std::vector<QString>>* names);

    void divideFilesBySize(QDir dir, std::map<size_t, std::vector<QString>>& sizes);

    void dealWithBigFiles(std::vector<QString> const& names);

    void runBigFiles(std::vector<std::vector<QString>> const& blocks);

    static int getBlocks(std::map<size_t, std::vector<QString>> const& sizes,
                   std::vector<std::vector<QString>>& smallNames,
                   std::vector<std::vector<QString>>& bigNames);

    static size_t addSizeBlock(std::vector<std::vector<QString>>& vec, std::vector<QString> const& block);

    QDir directory;

    static const qint64 MAX_FILE_SIZE = 1000000;

    QWidget * duplicatesReceiver;
    QWidget * errorsReceiver;

    bool stopped = false;

    int maxProgress = 0;
    int curProgress = 0;
};

#endif // DUPLICATESSEARCHER_H
