# Computer club management system
Simple Computer club management system made as test task for Yadro

Для сборки проекта достаточно обычных инструментов (например, cmake)

Важные детали:

1. В некоторых местах были выбраны хоть и не самые эффективные с точки зрения дополнительного времени и памяти решения, они были сделаны в угоду читаемости и расширяемости кода
2. В условии не были прописаны некоторые крайние случаи (например, обработка ошибок чтения файлов, двойная попытка записаться в очередь и т.п.). Я их продумал, однако не стал добавлять код для обработки, чтобы не усложнять код. Также я не знаю, насколько это критично для внешних тестов
3. К сожалению, я не успел настроить CI/CD
4. Я постарался поддержать работу как на Linux, так и на Windows
