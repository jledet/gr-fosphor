/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Sylvain Munaut <tnt@246tNt.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_FOSPHOR_QGLSURFACE_H
#define INCLUDED_GR_FOSPHOR_QGLSURFACE_H

#include <QGLWidget>
#include <QThread>

namespace gr {
  namespace fosphor {

    class qt_sink_c_impl;

    class Worker : public QObject
    {
      Q_OBJECT

     public:
      Worker(qt_sink_c_impl *sink);
      ~Worker();

     public Q_SLOTS:
      void process();

     Q_SIGNALS:
      void finished();
      void error(QString err);

     private:
      qt_sink_c_impl *d_sink;
    };


    class QGLSurface : public QGLWidget
    {
      Q_OBJECT

     private:
      qt_sink_c_impl *d_block;

     protected:
      void paintEvent(QPaintEvent *pe);
      void resizeEvent(QResizeEvent *re);
      void keyPressEvent(QKeyEvent *ke);
      void closeEvent(QCloseEvent * event);

     public:
      QGLSurface(QWidget *parent, qt_sink_c_impl *d_block);
      void handOver();
      QThread *d_thread;
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_QGLSURFACE_H */
