#ifndef RAMAN_H
#define RAMAN_H

#include "Util.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QGraphicsScene>
#include <QLayout>
#include <QListWidget>
#include <QMenu>
#include <QFileDialog>
#include <QPushButton>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QColor>
#include <QImage>
#include <QPixmap>	
#include <vector>

#include "Raman.h"
#include "Structure.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	std::vector<ItemData> dataVec;
	Raman *oriRaman;

	
private slots:
	void open();
	void addNewItem();
	void chooseColor();
	
	void show_contextmenu(const QPoint& pos);
	void show_savemenu(const QPoint&);
	void NLM();
	void display();
	void acceptAdd();
	void acceptNLM();
	

private:
	QGraphicsScene *scene;
	QGraphicsView *view;
	QVBoxLayout *inputLayout;
	QListWidget *listWidget;
	QMenu *listMenu;
	QMenu *saveMenu;
	QDialog *addDialog;
	QDialog *NLMDialog;
	QLineEdit *beginEdit;
	QLineEdit *endEdit;
	QColor color;

	QLineEdit *hEdit;
	QLineEdit *srEdit;
	QLineEdit *brEdit;
	QPushButton *NLMButton;

	QPushButton *DisplayButton;

	QPixmap *pixmap;

	void createFileMenu();
	void createStatusBar();
	void createInput();

	static const int sceneWidth = 320;
	static const int sceneHeight = 320;
};

#endif // RAMAN_H
