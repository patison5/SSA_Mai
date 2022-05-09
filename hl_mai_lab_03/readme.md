## лабораторая работа №3 по курсу Архитектура Программных систем


Запускаем в docker-compose кластер Apache Ignite:

- для архитектуры x86 используем образ apacheignite/ignite
- для архитектуры arm applem1support/ignite:2.12.0


Тестовый запрос для добавления в MySQL и кэш:

http://172.16.191.128/person?add&first_name=*Vasia*&last_name=*P**upkin*&title=*Mr*&email=*vasia@pupkin.com*

Тестовый запрос для поиска в кэше:

http://192.168.31.238/person?login=*some_login -* поиск записей (шаблон сквозное-чтение)
