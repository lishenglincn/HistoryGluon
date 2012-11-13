/* History Gluon
   Copyright (C) 2012 MIRACLE LINUX CORPORATION
 
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package com.miraclelinux.historygluon;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Semaphore;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class DataStreamThread extends Thread {

    /* -----------------------------------------------------------------------
     * Private constant
     * -------------------------------------------------------------------- */
    private int NUM_MAX_DATA_AT_ONCE = 100;

    /* -----------------------------------------------------------------------
     * Private member
     * -------------------------------------------------------------------- */
    private Log m_log = null;
    private BasicStorageDriver m_basicDriver = null;
    private Semaphore m_semaphore = null;
    private BlockingQueue<HistoryData> m_queue = null;
    private String m_key0 = null;
    private String m_key1 = null;

    /* -----------------------------------------------------------------------
     * Public Methods
     * -------------------------------------------------------------------- */
    public DataStreamThread(BasicStorageDriver basicDriver) {
        m_log = LogFactory.getLog(DataStreamThread.class); 
        m_basicDriver = basicDriver;
        m_semaphore = new Semaphore(1);
        m_semaphore.acquireUninterruptibly(); // This is never blocked.
    }

    public void openNewStream(BlockingQueue<HistoryData> queue,
                              String key0, String key1) {
        if (m_queue != null) {
            String msg;
            msg = "openNewStream() is called with non-null m_queue";
            throw new InternalCheckException(msg);
        }

        m_queue = queue;
        m_key0 = key0;
        m_key1 = key1;
        m_semaphore.release();
    }

    @Override
    public void run() {
        while (true) {
            try {
                m_semaphore.acquire();
                getDataStream();
            } catch (InterruptedException e) {
                // TODO: Implement
                e.printStackTrace();
                m_log.error(e);
            }
        }
    }

    /* -----------------------------------------------------------------------
     * Private Methods
     * -------------------------------------------------------------------- */
    private void getDataStream() {
        /*
        while (true) {
            HistoryDataSet dataSet = getDataSet(id, m_key0, m_key1,
                                                 NUM_MAX_DATA_AT_ONCE);
            int dataSize = dataSet.size();
            if (dataSize == 0)
                break;

            // push obtained data to the queue
            Iterator<HistoryData> it = dataSet.iterator();
            HistoryData history = null;
            while (it.hasNext()) {
                history = it.next();
                m_queue.put(history);
            }

            // break if this is the last chnuk.
            if (dataSize <  NUM_MAX_DATA_AT_ONCE)
                break;

            if (history.sec == 0xffffffff && history.ns == 0xffffffff)
                m_key0 =  m_basicStorage.makeKey(history.id+1, 0, 0);
            else
                m_key0 =  m_basicStorage.makeKey(history.id, history.sec,
                                                 history.ns + 1);
        }
        */
        putEndOfStream();
    }

    private void putEndOfStream() {
        HistoryData term = HistoryData.getEndOfStreamMarker();
        try {
            m_queue.put(term);
        } catch (InterruptedException e) {
            // TODO: implement exception
            e.printStackTrace();
            m_log.error(e);
        }
        m_queue = null;
    }
}