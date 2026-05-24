# Игра "GEMS" (на библиотеке SFML 3)

## Установка и запуск игры

### Шаг 1. Установка пакетного менеджера vcpkg

1. Откройте обычный **PowerShell** (или Командную строку) и перейдите в папку, куда хотите установить менеджер пакетов (например, в корень диска `C:\`).
2. Склонируйте официальный репозиторий vcpkg и соберите его:
   ```powershell
   cd C:\
   git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git)
   cd vcpkg
   .\bootstrap-vcpkg.bat
Шаг 2. Интеграция vcpkg в систему

Привяжите vcpkg к вашей Visual Studio:

PowerShell
.\vcpkg integrate install

Шаг 3. Установка библиотеки SFML 3
Перейдите в папку с vcpkg и установите графическую библиотеку под 64-битную архитектуру:

PowerShell
cd C:\vcpkg
.\vcpkg install sfml:x64-windows

Шаг 4. Клонирование репозитория с игрой
Шаг 5. Сборка и запуск игры
Найдите приложение Developer PowerShell for VS 2022 (оно устанавливается вместе с Visual Studio).

В консоли перейдите в папку со склонированной игрой:

Перейдите в папку вашего проекта:

PowerShell
cd "Путь_к_вашему_репозиторию"
Перед сборкой нам нужно сказать системе, где лежит ваш vcpkg. Выполните команду (замените путь на ваш, если он отличается):

PowerShell
$env:VCPKG_ROOT="C:\vcpkg"

Запустите готовую игру из этой же консоли:

PowerShell
.\x64\Debug\Lab3.exe

