#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), oriRaman(NULL), pixmap(NULL), saveMenu(NULL)
{
	createFileMenu();
	createStatusBar();

	QHBoxLayout *layout = new QHBoxLayout;

	createInput();
	layout->addLayout(inputLayout);
	// 拉曼成像图显示
	scene = new QGraphicsScene(this);
	scene->setSceneRect(QRectF(0, 0, sceneWidth, sceneHeight));
	view = new QGraphicsView(scene);
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(view, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(show_savemenu(const QPoint&)));
	layout->addWidget(view);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	setCentralWidget(widget);
	setWindowTitle(tr("Raman"));
}

MainWindow::~MainWindow()
{
	delete oriRaman;
	delete pixmap;
}

void MainWindow::createFileMenu()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *openAct = new QAction(tr("&Open..."), this);
	connect(openAct, SIGNAL(triggered(bool)), this, SLOT(open()));
	fileMenu->addAction(openAct);
	menuBar()->addSeparator();
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createInput()
{
	inputLayout = new QVBoxLayout;
	listWidget = new QListWidget(this);
	color = Qt::white;
	// 创建成像列表的右键菜单
	listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(listWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(show_contextmenu(const QPoint&)));
	listWidget->setFixedSize(200, 240);
	listMenu = NULL;
	inputLayout->addWidget(listWidget);
	
	NLMButton = new QPushButton("&NLM", this);
	NLMButton->setFixedSize(200, 40);
	connect(NLMButton, SIGNAL(clicked(bool)), this, SLOT(NLM()));
	inputLayout->addWidget(NLMButton);

	DisplayButton = new QPushButton("&display", this);
	DisplayButton->setFixedSize(200, 40);
	connect(DisplayButton, SIGNAL(clicked(bool)), this, SLOT(display()));
	inputLayout->addWidget(DisplayButton);

}

void MainWindow::show_contextmenu(const QPoint& pos)
{
	if (listMenu)//保证同时只存在一个menu，及时释放内存
	{
		delete listMenu;
		listMenu = NULL;
	}
	listMenu = new QMenu(listWidget);

	QAction *addNewItemAction = listMenu->addAction(tr("Add"));
	connect(addNewItemAction, SIGNAL(triggered(bool)), this, SLOT(addNewItem()));

	QListWidgetItem * target;
	if (target = listWidget->itemAt(pos)) {
		QAction *deleteItemAction = listMenu->addAction(tr("Delete"));
		connect(deleteItemAction, &QAction::triggered, this, [=](){
			int row = listWidget->row(target);
			dataVec.erase(dataVec.begin() + row);
			listWidget->takeItem(row);
		});
		QAction *alterItemAction = listMenu->addAction(tr("Modify"));
		connect(alterItemAction, &QAction::triggered, this, [=]() {
			int row = listWidget->row(target);
			int oldSize = dataVec.size();

			addDialog = new QDialog(this);
			addDialog->setWindowFlags(Qt::Drawer);
			QGridLayout *addLayout = new QGridLayout;

			QLabel *beginLable = new QLabel(tr("Raman shift begin: "));
			beginEdit = new QLineEdit();
			beginEdit->setValidator(new QIntValidator(0, 4000.0, this));
			beginEdit->setText(QString("%1").arg(dataVec[row].begin));
			addLayout->addWidget(beginLable, 0, 0);
			addLayout->addWidget(beginEdit, 0, 1);

			QLabel *endLable = new QLabel(tr("Raman shift end: "));
			endEdit = new QLineEdit();
			endEdit->setValidator(new QIntValidator(0, 4000.0, this));
			endEdit->setText(QString("%1").arg(dataVec[row].end));
			addLayout->addWidget(endLable, 1, 0);
			addLayout->addWidget(endEdit, 1, 1);

			QPushButton *ColorButton = new QPushButton("&Choose color", this);
			connect(ColorButton, SIGNAL(clicked(bool)), this, SLOT(chooseColor()));
			addLayout->addWidget(ColorButton, 2, 0, 1, 2);

			QPushButton *OKButton = new QPushButton("&OK", this);
			connect(OKButton, SIGNAL(clicked(bool)), this, SLOT(acceptAdd()));
			addLayout->addWidget(OKButton, 3, 0);
			QPushButton *cancelButton = new QPushButton("&Cancel", this);
			connect(cancelButton, SIGNAL(clicked(bool)), addDialog, SLOT(reject()));
			addLayout->addWidget(cancelButton, 3, 1);

			addDialog->setLayout(addLayout);
			addDialog->exec();
			if (oldSize != dataVec.size()) {
				dataVec[row] = dataVec[dataVec.size() - 1];
				dataVec.pop_back();
				listWidget->takeItem(row);
				listWidget->insertItem(row, listWidget->takeItem(listWidget->count() - 1));
			}
		});
	}

	listMenu->exec(QCursor::pos());//在当前鼠标位置显示
}
void MainWindow::show_savemenu(const QPoint&)
{
	if (!pixmap) {
		return;
	}
	if (saveMenu)//保证同时只存在一个menu，及时释放内存
	{
		delete saveMenu;
		saveMenu = NULL;
	}
	saveMenu = new QMenu(view);

	QAction *saveAsAction = saveMenu->addAction(tr("Save picture"));
	QString fileName;
	connect(saveAsAction, &QAction::triggered, this, [&]() { 
		QDialog *saveDialog = new QDialog(this);
		saveDialog->setWindowFlags(Qt::Drawer);
		QGridLayout *saveLayout = new QGridLayout;

		QLabel *heightLable = new QLabel(tr("Height: "));
		QLineEdit *heightEdit = new QLineEdit();
		heightEdit->setText("320");
		heightEdit->setValidator(new QIntValidator(0, 1000, this));
		saveLayout->addWidget(heightLable, 0, 0);
		saveLayout->addWidget(heightEdit, 0, 1);

		QLabel *widthLable = new QLabel(tr("Width: "));
		QLineEdit *widthEdit = new QLineEdit();
		widthEdit->setText("320");
		widthEdit->setValidator(new QIntValidator(0, 1000, this));
		saveLayout->addWidget(widthLable, 1, 0);
		saveLayout->addWidget(widthEdit, 1, 1);

		QPushButton *OKButton = new QPushButton("&OK", this);
		connect(OKButton, &QPushButton::clicked, this, [&]() {
			int newWidth = widthEdit->text().toInt();
			int newHeight = heightEdit->text().toInt();
			saveDialog->close();
			fileName = QFileDialog::getSaveFileName(view, "Save", "", "Image Files(*.jpg *.png)");
			if (!fileName.isNull()) {
				QPixmap scaledPixmap = pixmap->scaled(newWidth, newHeight, Qt::KeepAspectRatio);
				scaledPixmap.save(fileName);
			}
		});
		saveLayout->addWidget(OKButton, 2, 0);
		QPushButton *cancelButton = new QPushButton("&Cancel", this);
		connect(cancelButton, SIGNAL(clicked(bool)), saveDialog, SLOT(reject()));
		saveLayout->addWidget(cancelButton, 2, 1);
		saveDialog->setLayout(saveLayout);
		saveDialog->exec();
	});
	saveMenu->exec(QCursor::pos());//在当前鼠标位置显示
}

void MainWindow::addNewItem()
{
	addDialog = new QDialog(this);
	addDialog->setWindowFlags(Qt::Drawer);
	QGridLayout *addLayout = new QGridLayout;

	QLabel *beginLable = new QLabel(tr("Raman shift begin: "));
	beginEdit = new QLineEdit();
	beginEdit->setValidator(new QIntValidator(0, 4000.0, this));
	beginEdit->setText("771");
	addLayout->addWidget(beginLable, 0, 0);
	addLayout->addWidget(beginEdit, 0, 1);

	QLabel *endLable = new QLabel(tr("Raman shift end: "));
	endEdit = new QLineEdit();
	endEdit->setValidator(new QIntValidator(0, 4000.0, this));
	endEdit->setText("801");
	addLayout->addWidget(endLable, 1, 0);
	addLayout->addWidget(endEdit, 1, 1);

	QPushButton *ColorButton = new QPushButton("&Choose color", this);
	connect(ColorButton, SIGNAL(clicked(bool)), this, SLOT(chooseColor()));
	addLayout->addWidget(ColorButton, 2, 0, 1, 2);

	QPushButton *OKButton = new QPushButton("&OK", this);
	connect(OKButton, SIGNAL(clicked(bool)), this, SLOT(acceptAdd()));
	addLayout->addWidget(OKButton, 3, 0);
	QPushButton *cancelButton = new QPushButton("&Cancel", this);
	connect(cancelButton, SIGNAL(clicked(bool)), addDialog, SLOT(reject()));
	addLayout->addWidget(cancelButton, 3, 1);

	addDialog->setLayout(addLayout);
	addDialog->exec();
}
void MainWindow::chooseColor()
{
	color = QColorDialog::getColor(color, this);
}
void MainWindow::acceptAdd()
{
	ItemData data;
	data.begin = beginEdit->text().toDouble();
	data.end = endEdit->text().toDouble();
	if (data.end > data.begin) {
		data.red = color.red();
		data.green = color.green();
		data.blue = color.blue();
		dataVec.push_back(data);

		QString msg = QString("[%1, %2]").arg(beginEdit->text(), endEdit->text());
		QListWidgetItem *item = new QListWidgetItem(msg);
		item->setTextAlignment(Qt::AlignHCenter);
		item->setBackgroundColor(color);
		listWidget->addItem(item);
		addDialog->close();
	}
}

void MainWindow::open()
{
	QString file_name = QFileDialog::getOpenFileName(this,
													 tr("Open File"),
													 "",
													 tr("Textfile(*.txt);;Datafile(*.dat);;Allfile(*.*)"),
													 0);
	if (file_name.isNull()) {
		return;
	}
	QDialog *openDialog = new QDialog(this);
	openDialog->setWindowFlags(Qt::Drawer);
	QGridLayout *openLayout = new QGridLayout;

	QLabel *beginLable = new QLabel(tr("Raman scattering begin: "));
	QLineEdit *wholeBeginEdit = new QLineEdit;
	wholeBeginEdit->setText("532.26");
	wholeBeginEdit->setValidator(new QIntValidator(0, 4000, this));
	openLayout->addWidget(beginLable, 0, 0);
	openLayout->addWidget(wholeBeginEdit, 0, 1);

	QLabel *endLable = new QLabel(tr("Raman scattering end: "));
	QLineEdit *wholeEndEdit = new QLineEdit();
	wholeEndEdit->setText("665.94");
	wholeEndEdit->setValidator(new QIntValidator(0, 4000, this));
	openLayout->addWidget(endLable, 1, 0);
	openLayout->addWidget(wholeEndEdit, 1, 1);

	QLabel *heightLable = new QLabel(tr("Height: "));
	QLineEdit *heightEdit = new QLineEdit();
	heightEdit->setText("32");
	heightEdit->setValidator(new QIntValidator(0, 128, this));
	openLayout->addWidget(heightLable, 2, 0);
	openLayout->addWidget(heightEdit, 2, 1);

	QLabel *widthLable = new QLabel(tr("Width: "));
	QLineEdit *widthEdit = new QLineEdit();
	widthEdit->setText("32");
	widthEdit->setValidator(new QIntValidator(0, 128, this));
	openLayout->addWidget(widthLable, 3, 0);
	openLayout->addWidget(widthEdit, 3, 1);
	

	QPushButton *OKButton = new QPushButton("&OK", this);
	connect(OKButton, &QPushButton::clicked, this, [&]() { 
		double begin = wholeBeginEdit->text().toDouble();
		double end = wholeEndEdit->text().toDouble();
		int width = widthEdit->text().toInt();
		int height = heightEdit->text().toInt();
		std::string filename = file_name.toStdString();		
		oriRaman = new Raman(filename, height, width, begin, end);
		openDialog->close();
	});
	openLayout->addWidget(OKButton, 4, 0);
	QPushButton *cancelButton = new QPushButton("&Cancel", this);
	connect(cancelButton, SIGNAL(clicked(bool)), openDialog, SLOT(reject()));
	openLayout->addWidget(cancelButton, 4, 1);

	openDialog->setLayout(openLayout);
	openDialog->exec();
}

void MainWindow::NLM()
{
	if (!oriRaman) {
		return ;
	}
	NLMDialog = new QDialog(this);
	NLMDialog->setWindowFlags(Qt::Drawer);
	QGridLayout *NLMLayout = new QGridLayout;

	QLabel *hLable = new QLabel(tr("degree: "));
	hEdit = new QLineEdit();
	hEdit->setValidator(new QIntValidator(0, 20, this));
	hEdit->setText("1");
	NLMLayout->addWidget(hLable, 0, 0);
	NLMLayout->addWidget(hEdit, 0, 1);

	QLabel *srLable = new QLabel(tr("search windows radius: "));
	srEdit = new QLineEdit();
	srEdit->setValidator(new QIntValidator(0, 10, this));
	srEdit->setText("3");
	NLMLayout->addWidget(srLable, 1, 0);
	NLMLayout->addWidget(srEdit, 1, 1);

	QLabel *brLable = new QLabel(tr("image block radius: "));
	brEdit = new QLineEdit();
	brEdit->setValidator(new QIntValidator(0, 10, this));
	brEdit->setText("5");
	NLMLayout->addWidget(brLable, 2, 0);
	NLMLayout->addWidget(brEdit, 2, 1);

	QPushButton *OKButton = new QPushButton("&OK", this);
	connect(OKButton, SIGNAL(clicked(bool)), this, SLOT(acceptNLM()));
	NLMLayout->addWidget(OKButton, 3, 0);
	QPushButton *cancelButton = new QPushButton("&Cancel", this);
	connect(cancelButton, SIGNAL(clicked(bool)), NLMDialog, SLOT(reject()));
	NLMLayout->addWidget(cancelButton, 3, 1);

	NLMDialog->setLayout(NLMLayout);
	NLMDialog->exec();
}
void MainWindow::display()
{
	if (!oriRaman) {
		return;
	}
	Bitmap bg = oriRaman->CreateBitmap(0, 0, Pixel(RGB(0, 0, 0)));
	double delta = oriRaman->endLambda - oriRaman->beginLambda;
	for (int i = 0; i < dataVec.size(); ++i)
	{
		// 将位移转换为波长
		double beginWL = 1e7 / (1e7 / 532.0f - dataVec[i].begin);
		double endWL = 1e7 / (1e7 / 532.0f - dataVec[i].end);
		int begin = (beginWL - oriRaman->beginLambda) / delta * oriRaman->spectrumSize;
		int end = (endWL - oriRaman->beginLambda) / delta * oriRaman->spectrumSize;
		Pixel pixel;
		pixel.red = dataVec[i].red;
		pixel.green = dataVec[i].green;
		pixel.blue = dataVec[i].blue;
		Bitmap result = oriRaman->BlendBitmap(begin, end, pixel, bg);
		bg = result;
	}
	bg.SaveAs("temp.bmp");
	delete pixmap;
	pixmap = new QPixmap("temp.bmp");
	QPixmap scaledPixmap = pixmap->scaled(sceneWidth, sceneHeight, Qt::KeepAspectRatio);
	scene->addPixmap(scaledPixmap);
}

void MainWindow::acceptNLM()
{
	int d = brEdit->text().toInt();
	int sr = srEdit->text().toInt();
	int h = hEdit->text().toInt();
	oriRaman->DoNLM(d, sr, h);
	NLMDialog->close();
}


