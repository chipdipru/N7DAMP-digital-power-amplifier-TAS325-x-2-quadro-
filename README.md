N7 digital power amplifier TAS325 x 2 (quadro). Подключение двух усилителей мощности TAS3251 
Это проект для создания USB – I2S - DAC  цифрового усилителя мощности. 
Четыре канала по 175 Вт.
В проекте задействованы:
1.	USB / I2S преобразователь 32bit/96kHz, <a class="link" href="https://www.chipdip.ru/product0/9000569733"> SUPER PRIME chipdip </a>, USB Hi-Res Audio, квадро, построенный на микроконтроллере STM32F446RC
2. <a class="link" href="https://www.chipdip.ru/product/rdc2-0032">RDC2-0032</a>. Управляющий элемент для проектов цифровой обработки звука. Микроконтроллер STM32F042
3.	Два цифровых усилителя мощности <a class="link" href="https://www.chipdip.ru/product/rdc2-0050">RDC2-0050</a>. 175Вт Stereo, TAS3251
4.	Четыре <a class="link" href="https://www.chipdip.ru/product/lcf05">LCF05</a>, Выходной LC-фильтр второго порядка для усилителей мощности класса D.

<h2>Инструкция по сборке</h2>
В этом проекте инициализируются сразу два усилителя TAS3251. С помощью трех потенциометров можно регулировать громкость всех четырех каналов (RV1) и балансы: лево/право (RV2), вперёд/назад (RV3).

Соедините все модули согласно схеме. Резисторы, подключаемые к RDC2-0032 должны быть с линейной характеристикой сопротивлением от 10 до 100 кОм.
Для прошивки RDC2-0032 скачайте программу <a class="link" href="https://static.chipdip.ru/lib/465/DOC002465470.zip">DfuSeDemo</a> от STMicroelectronics.   Установите её на свой компьютер. Запустите программу.<br>
На плате  RDC2-0032 найдите контакты Boot и замкните их на время программирования. Подключите RDC2-0032 к порту USB. 
В DfuSeDemo в разделе “Upgrade or Verify Action” нажмите “Choose”. 
<p><a class="galery" href="https://static.chipdip.ru/lib/504/DOC004504421.jpg"><img alt="DFU" src="https://static.chipdip.ru/lib/504/DOC004504742.jpg" /></a></p>
Выберите файл прошивки с расширением.dfu”.
<p><a class="galery" href="https://static.chipdip.ru/lib/504/DOC004504421.jpg"><img alt="DFU" src="https://static.chipdip.ru/lib/504/DOC004504743.jpg" /></a></p>
Выберите опцию “Verify after download”. Нажмите кнопку “Upgrade”. В появившемся окне нажмите «Да». Начнется загрузка программы в устройство.
<p><a class="galery" href="https://static.chipdip.ru/lib/504/DOC004504421.jpg"><img alt="DFU" src="https://static.chipdip.ru/lib/504/DOC004504744.jpg" /></a></p>
Дождитесь окончания загрузки. 
Загрузка программы завершена, устройство готово к эксплуатации.
<p><a class="galery" href="https://static.chipdip.ru/lib/504/DOC004504421.jpg"><img alt="DFU" src="https://static.chipdip.ru/lib/504/DOC004504745.jpg" /></a></p>
Блок питания для всей конструкции рекомендуется – однополярный 36В / 10А. 
USB – I2S конвертер установите в режим квадро. Джамперы 13 и 14 установлены. 
Подключите USB – I2S конвертер к компьютеру. Укажите что ваше устройство воспроизведения – USB I2S PRIME SUPER Quadro 4.0. Выберите разрядность и частоту дискретизации и включайте музыку или играйте в игры.

<h2>Схема подключения</h2>
<p><a class="galery" href="https://static.chipdip.ru/lib/504/DOC004504421.jpg"><img alt="Схема" src="https://static.chipdip.ru/lib/504/DOC004504422.jpg" /></a></p>
