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

#include <QtEvents>
#include "QGLSurface.h"

#include "qt_sink_c_impl.h"

#include <stdio.h>
#include <chrono>

namespace gr {
  namespace fosphor {

Worker::Worker(qt_sink_c_impl *sink) {
	d_sink = sink;
}

Worker::~Worker() {
}

void Worker::process() {
	d_sink->worker();
}


QGLSurface::QGLSurface(QWidget *parent, qt_sink_c_impl *block)
  : QGLWidget(parent), d_block(block)
{
	this->setFocusPolicy(Qt::StrongFocus);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void
QGLSurface::handOver() {

	d_thread = new QThread();
	Worker *worker = new Worker(d_block);
	worker->moveToThread(d_thread);

	connect(d_thread, SIGNAL(started()), worker, SLOT(process()));

	context()->doneCurrent();
	context()->moveToThread(d_thread);

	d_thread->start();
}

void
QGLSurface::closeEvent(QCloseEvent *event) {
	d_block->d_active = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	event->accept();
}

void
QGLSurface::paintEvent(QPaintEvent *pe)
{
	/* Don't do anything */

	/*
	 * The default implementation calls makeCurrent but here we want
	 * _other_ threads to be current, so we need a dummy empty impl
	 * for the paintEvent
	 */
}

void
QGLSurface::resizeEvent(QResizeEvent *re)
{
	/*
	 * The default implementation calls makeCurrent but here we want
	 * _other_ threads to be current, so don't do that !
	 */

	/* Call back to main block */
	this->d_block->cb_reshape(re->size().width(), re->size().height());
}

void
QGLSurface::keyPressEvent(QKeyEvent *ke)
{
	switch (ke->key()) {
	case Qt::Key_Up:
		this->d_block->execute_ui_action(qt_sink_c_impl::REF_DOWN);
		break;
	case Qt::Key_Down:
		this->d_block->execute_ui_action(qt_sink_c_impl::REF_UP);
		break;
	case Qt::Key_Left:
		this->d_block->execute_ui_action(qt_sink_c_impl::DB_PER_DIV_DOWN);
		break;
	case Qt::Key_Right:
		this->d_block->execute_ui_action(qt_sink_c_impl::DB_PER_DIV_UP);
		break;
	case Qt::Key_Z:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_TOGGLE);
		break;
	case Qt::Key_W:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_WIDTH_UP);
		break;
	case Qt::Key_S:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_WIDTH_DOWN);
		break;
	case Qt::Key_D:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_CENTER_UP);
		break;
	case Qt::Key_A:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_CENTER_DOWN);
		break;
	case Qt::Key_Q:
		this->d_block->execute_ui_action(qt_sink_c_impl::RATIO_UP);
		break;
	case Qt::Key_E:
		this->d_block->execute_ui_action(qt_sink_c_impl::RATIO_DOWN);
		break;
	case Qt::Key_Space:
		this->d_block->execute_ui_action(qt_sink_c_impl::FREEZE_TOGGLE);
		break;
	}
}

  } /* namespace fosphor */
} /* namespace gr */
