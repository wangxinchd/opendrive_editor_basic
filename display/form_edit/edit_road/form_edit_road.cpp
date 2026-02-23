#include "form_edit_road.h"
#include "ui_form_edit_road.h"

FormEditRoad::FormEditRoad(QWidget *parent)
    : QWidget(parent), ui(new Ui::FormEditRoad) {
  ui->setupUi(this);
}

FormEditRoad::~FormEditRoad() { delete ui; }
