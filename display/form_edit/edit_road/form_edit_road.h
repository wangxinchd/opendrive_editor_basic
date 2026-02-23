#ifndef FORM_EDIT_ROAD_H
#define FORM_EDIT_ROAD_H

#include <QWidget>

namespace Ui {
class FormEditRoad;
}

class FormEditRoad : public QWidget {
  Q_OBJECT

public:
  explicit FormEditRoad(QWidget *parent = nullptr);
  ~FormEditRoad();

private:
  Ui::FormEditRoad *ui;
};

#endif // FORM_EDIT_ROAD_H
