# lab6

## 1. Замена std::unique_ptr на std::shared_ptr
   - Было: `std::vector<std::unique_ptr<Contact>> contacts;`
   - Стало: `std::vector<std::shared_ptr<Contact>> contacts;`

## 2. Добавлен отдельный вектор для групп
   - Добавлено: `std::vector<std::shared_ptr<GroupContact>> groups;`

## 3. Новые методы в классе `GroupContact`
   - `addMember(std::shared_ptr<Contact> contact)`
   - `removeMember(std::shared_ptr<Contact> contact)`
   - `hasMember(std::shared_ptr<Contact> contact) const`
   - `getMemberCount() const`
   - `getMembers() const`
   - Добавлено поле `std::vector<std::shared_ptr<Contact>> members;`

## 4. Изменена фабрика
   - Было: `static std::unique_ptr<Contact> create(QString type);`
   - Стало: `static std::shared_ptr<Contact> create(QString type);`

## 5. В методе `addContact` добавлена обработка групп

## 6. Тестовые данные теперь демонстрируют, что один контакт может быть в нескольких группах
```
auto friends = std::make_shared<GroupContact>("Друзья", "Школьные друзья", 2);
friends->addMember(person1);
friends->addMember(person2);

auto work = std::make_shared<GroupContact>("Работа", "Коллеги", 2);
work->addMember(person2);  // Мария и в друзьях, и на работе
work->addMember(company);
```

