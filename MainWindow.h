#pragma once
#include <QMainWindow>

namespace Ui {
  class MainWindow;
}


class MainWindow : public QMainWindow
{
Q_OBJECT
  Ui::MainWindow* ui;

public:
  explicit MainWindow(QWidget* parent = 0);
};
