#include "mainwindow.h"
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>

// Реализация методов PersonContact
PersonContact::PersonContact(QString n, QString p, QString e)
    : name(n), phone(p), email(e) {}

QString PersonContact::getName() const { return name; }
QString PersonContact::getInfo() const { return phone + "\n" + email; }
QString PersonContact::getType() const { return "Человек"; }
QString PersonContact::toText() const { return "P:" + name + ";" + phone + ";" + email; }
void PersonContact::setData(QString n, QString p, QString e) { name = n; phone = p; email = e; }

// Реализация методов CompanyContact
CompanyContact::CompanyContact(QString n, QString p, QString a)
    : name(n), phone(p), address(a) {}

QString CompanyContact::getName() const { return name; }
QString CompanyContact::getInfo() const { return phone + "\n" + address; }
QString CompanyContact::getType() const { return "Компания"; }
QString CompanyContact::toText() const { return "C:" + name + ";" + phone + ";" + address; }
void CompanyContact::setData(QString n, QString p, QString a) { name = n; phone = p; address = a; }

// Реализация методов GroupContact
GroupContact::GroupContact(QString n, QString d, int c)
    : name(n), desc(d), count(c) {}

QString GroupContact::getName() const { return name; }
QString GroupContact::getInfo() const { return desc + "\n" + QString::number(members.size()) + " участников"; }
QString GroupContact::getType() const { return "Группа"; }
QString GroupContact::toText() const { return "G:" + name + ";" + desc + ";" + QString::number(members.size()); }
void GroupContact::setData(QString n, QString d, int c) { name = n; desc = d; count = c; }

void GroupContact::addMember(std::shared_ptr<Contact> contact) {
    if (contact && !hasMember(contact)) {
        members.push_back(contact);
    }
}

void GroupContact::removeMember(std::shared_ptr<Contact> contact) {
    auto it = std::find(members.begin(), members.end(), contact);
    if (it != members.end()) {
        members.erase(it);
    }
}

bool GroupContact::hasMember(std::shared_ptr<Contact> contact) const {
    return std::find(members.begin(), members.end(), contact) != members.end();
}

size_t GroupContact::getMemberCount() const {
    return members.size();
}

std::vector<std::shared_ptr<Contact>> GroupContact::getMembers() const {
    return members;
}

// Реализация методов EmergencyContact
EmergencyContact::EmergencyContact(QString n, QString p, QString r)
    : name(n), phone(p), relation(r) {}

QString EmergencyContact::getName() const { return name; }
QString EmergencyContact::getInfo() const { return phone + "\n" + relation; }
QString EmergencyContact::getType() const { return "ЧС"; }
QString EmergencyContact::toText() const { return "E:" + name + ";" + phone + ";" + relation; }
void EmergencyContact::setData(QString n, QString p, QString r) { name = n; phone = p; relation = r; }

// Реализация фабрики
std::shared_ptr<Contact> ContactFactory::create(QString type) {
    if (type == "Человек") return std::make_shared<PersonContact>();
    if (type == "Компания") return std::make_shared<CompanyContact>();
    if (type == "Группа") return std::make_shared<GroupContact>();
    if (type == "ЧС") return std::make_shared<EmergencyContact>();
    return nullptr;
}

std::shared_ptr<Contact> ContactFactory::fromText(QString text) {
    if (text.startsWith("P:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_shared<PersonContact>(parts[0], parts[1], parts[2]);
    }
    if (text.startsWith("C:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_shared<CompanyContact>(parts[0], parts[1], parts[2]);
    }
    if (text.startsWith("G:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_shared<GroupContact>(parts[0], parts[1], parts[2].toInt());
    }
    if (text.startsWith("E:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_shared<EmergencyContact>(parts[0], parts[1], parts[2]);
    }
    return nullptr;
}

// Реализация MainWindow
MainWindow::MainWindow() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    QWidget* left = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(left);
    left->setMaximumWidth(300);

    // Поиск и фильтр
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Поиск");
    typeBox = new QComboBox();
    typeBox->addItems({"Все", "Человек", "Компания", "Группа", "ЧС"});
    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(typeBox);
    leftLayout->addLayout(searchLayout);

    list = new QListWidget();
    leftLayout->addWidget(list);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    addBtn = new QPushButton("+ Добавить");
    delBtn = new QPushButton("- Удалить");
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    leftLayout->addLayout(btnLayout);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    saveBtn = new QPushButton("Сохранить");
    loadBtn = new QPushButton("Загрузить");
    fileLayout->addWidget(saveBtn);
    fileLayout->addWidget(loadBtn);
    leftLayout->addLayout(fileLayout);

    QWidget* right = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(right);

    QLabel* titleLabel = new QLabel("Детали контакта:");
    rightLayout->addWidget(titleLabel);

    details = new QTextEdit();
    details->setReadOnly(true);
    rightLayout->addWidget(details);

    mainLayout->addWidget(left);
    mainLayout->addWidget(right);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::deleteContact);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(list, &QListWidget::currentRowChanged, this, &MainWindow::showContact);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::filter);
    connect(typeBox, &QComboBox::currentTextChanged, this, &MainWindow::filter);

    auto person1 = std::make_shared<PersonContact>("Иван Петров", "123-45-67", "ivan@mail.ru");
    auto person2 = std::make_shared<PersonContact>("Мария Сидорова", "234-56-78", "maria@mail.ru");
    auto company = std::make_shared<CompanyContact>("ООО что-то", "495-111-22-33", "Москва");
    auto emergency = std::make_shared<EmergencyContact>("Скорая помощь", "103", "Медицина");

    contacts.push_back(person1);
    contacts.push_back(person2);
    contacts.push_back(company);
    contacts.push_back(emergency);

    auto friends = std::make_shared<GroupContact>("Друзья", "Школьные друзья", 2);
    friends->addMember(person1);
    friends->addMember(person2);

    auto work = std::make_shared<GroupContact>("Работа", "Коллеги", 2);
    work->addMember(person2);
    work->addMember(company);

    groups.push_back(friends);
    groups.push_back(work);

    // Добавляем группы в общий список контактов для отображения
    contacts.push_back(friends);
    contacts.push_back(work);

    updateList();

    setWindowTitle("Редактор контактов");
    resize(700, 500);
}

void MainWindow::addContact() {
    QString type = typeBox->currentText();
    if (type == "Все") type = "Человек";

    auto contact = ContactFactory::create(type);
    if (!contact) return;

    QStringList labels;
    QStringList values;

    if (type == "Человек") {
        labels = {"Имя:", "Телефон:", "Email:"};
    } else if (type == "Компания") {
        labels = {"Название:", "Телефон:", "Адрес:"};
    } else if (type == "Группа") {
        labels = {"Название:", "Описание:", "Кол-во участников:"};
    } else {
        labels = {"Имя:", "Телефон:", "Отношение:"};
    }

    for (const QString& label : labels) {
        bool ok;
        QString value = QInputDialog::getText(this, "Новый контакт", label, QLineEdit::Normal, "", &ok);
        if (!ok) return;
        values << value;
    }

    if (type == "Person") {
        std::dynamic_pointer_cast<PersonContact>(contact)->setData(values[0], values[1], values[2]);
        contacts.push_back(contact);
    } else if (type == "Company") {
        std::dynamic_pointer_cast<CompanyContact>(contact)->setData(values[0], values[1], values[2]);
        contacts.push_back(contact);
    } else if (type == "Group") {
        auto group = std::dynamic_pointer_cast<GroupContact>(contact);
        group->setData(values[0], values[1], values[2].toInt());
        contacts.push_back(group);
        groups.push_back(group);
    } else {
        std::dynamic_pointer_cast<EmergencyContact>(contact)->setData(values[0], values[1], values[2]);
        contacts.push_back(contact);
    }

    updateList();
}

void MainWindow::deleteContact() {
    int row = list->currentRow();
    if (row < 0) return;

    Contact* c = filtered.empty() ? contacts[row].get() : filtered[row];

    // Если удаляем группу, удаляем её из списка групп
    if (c->getType() == "Группа") {
        auto it = std::find_if(groups.begin(), groups.end(),
                               [c](auto& ptr) { return ptr.get() == c; });
        if (it != groups.end()) {
            groups.erase(it);
        }
    }

    if (!filtered.empty()) {
        auto it = std::find_if(contacts.begin(), contacts.end(),
                               [c](auto& ptr) { return ptr.get() == c; });
        if (it != contacts.end()) contacts.erase(it);
        filtered.clear();
        filter();
    } else {
        contacts.erase(contacts.begin() + row);
    }

    updateList();
    details->clear();
}

void MainWindow::saveFile() {
    QString file = QFileDialog::getSaveFileName(this, "Сохранить контакты", "", "Text files (*.txt)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&f);
    for (auto& c : contacts) {
        out << c->toText() << "\n";
    }
    f.close();

    QMessageBox::information(this, "Готово", "Контакты сохранены!");
}

void MainWindow::loadFile() {
    QString file = QFileDialog::getOpenFileName(this, "Загрузить контакты", "", "Text files (*.txt)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
        return;
    }

    contacts.clear();
    groups.clear();

    QTextStream in(&f);
    while (!in.atEnd()) {
        auto c = ContactFactory::fromText(in.readLine());
        if (c) {
            contacts.push_back(c);
            if (c->getType() == "Группа") {
                groups.push_back(std::dynamic_pointer_cast<GroupContact>(c));
            }
        }
    }
    f.close();

    filtered.clear();
    updateList();
    details->clear();
    QMessageBox::information(this, "Готово", "Контакты загружены!");
}

void MainWindow::showContact(int row) {
    if (row < 0) {
        details->clear();
        return;
    }

    Contact* c = filtered.empty() ? contacts[row].get() : filtered[row];

    QString text = "" + c->getName() + "\n\n";
    text += "Тип: " + c->getType() + "\n\n";
    text += c->getInfo();

    // Для группы показываем участников
    if (c->getType() == "Группа") {
        // Находим группу в векторе groups
        for (auto& group : groups) {
            if (group.get() == c) {
                text += "\n\nУчастники группы:\n";
                for (auto& member : group->getMembers()) {
                    text += " " + member->getName() + "\n";
                }
                break;
            }
        }
    }

    details->setText(text);
}

void MainWindow::filter() {
    QString search = searchBox->text().toLower();
    QString type = typeBox->currentText();

    filtered.clear();

    for (auto& c : contacts) {
        bool matchSearch = search.isEmpty() || c->getName().toLower().contains(search);
        bool matchType = type == "Все" || c->getType() == type;

        if (matchSearch && matchType) {
            filtered.push_back(c.get());
        }
    }

    updateList();
}

void MainWindow::updateList() {
    list->clear();

    if (filtered.empty()) {
        for (auto& c : contacts) {
            list->addItem(c->getName());
        }
    } else {
        for (Contact* c : filtered) {
            list->addItem(c->getName());
        }
    }
}
