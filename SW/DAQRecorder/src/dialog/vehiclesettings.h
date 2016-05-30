#ifndef VEHICLESETTINGS_H
#define VEHICLESETTINGS_H

#include <QDialog>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>

namespace Ui {
    class VehicleSettings;
}

class VehicleSettings : public QDialog{
    Q_OBJECT

public:
    explicit VehicleSettings(QWidget *parent = 0);
    ~VehicleSettings();
    int exec();

private slots:
    void reloadVehicleList();
    void selectVehicle();
    void selectVehicleImg();
    void selectVehicleModel();

    void addVehicle();
    void removeVehicle();
    void save();

private:
    Ui::VehicleSettings *ui;
};

#endif // VEHICLESETTINGS_H
