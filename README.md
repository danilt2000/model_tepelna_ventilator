<h1>Modul Tepelné soustavy s ventilátorem</h1>
<img src="https://user-images.githubusercontent.com/75219332/204537564-c71c29d1-fcb1-4cd8-9490-ae8815cd5ecc.png"></img>
<h2>Vytvořte program pro řízení otáček ventilátoru dle aktuální teploty chlazeného zařízení.</h2>
<p>Program bude využívat model tepelné soustavy s ventilátorem připojený k vývojovému kitu.</p>
<pre>Program by měl mít následující funkce:
· Možnost ručního řízení otáček ventilátoru nezávisle na teplotě.
· Možnost automatického řízení otáček v závislosti na teplotě jednoduchým dvoupolohovým
regulátorem tak, aby teplota chlazeného zařízení byla udržována na uživatelem nastavené hodnotě. Při
vyšší teplotě než je nastavená, poběží ventilátor na plný výkon, při nižší teplotě na 30 % výkonu.
Pozor, aby nedocházelo k rychlému kmitání kolem rozhodovací úrovně, bude možno nastavit hysterezi
regulátoru.
· Možnost automatického řízení otáček pomocí Proporcionálně-Sumačního (PS) regulátoru tak, aby
teplota chlazeného zařízení byla udržována na uživatelem nastavené hodnotě.
Poznámka: Modul tepelné soustavy s ventilátorem umožňuje ručně ovládat ohřev topného elementu a
pomocí mikropočítače snímat jeho teplotu a řídit ventilátor.
Ovládání programu:
Program se bude ovládat pomocí tlačítek a potenciometru na vývojovém kitu. Funkci ovládacích prvků zvolte
podle uvážení, tak aby bylo ovládání co nejpohodlnější a intuitivní. Ovládání programu popište v dokumentaci.
Informace na displeji:
Na displeji vývojového kitu by měly být zobrazeny minimálně tyto informace:
· Aktuální režim
· Otáčky ventilátoru
· Aktuální střída PWM v procentech
· Teplota topného elementu a žádaná teplota (pokud má v daném režimu smysl)
· Informace potřebné pro pohodlné nastavování programu
Vhodně také využijte LED na vývojovém kitu pro indikaci stavu programu.
Ovládání přes sériovou linku:
Ovládání z PC přes sériovou linku může umožňovat ovládat stejné funkce jako pomocí tlačítek na kitu. Program
také může na sériovou linku odesílat aktuální údaje podobně, jako se zobrazují na displeji kitu.
Komunikace s PC může představovat samostatný režim, do kterého program přejde vámi zvoleným způsobem
(např. stiskem tlačítka na kitu). V tomto režimu pak bude ovládání programu probíhat přes sériovou linku
pomocí textových příkazů. Názvy a formát příkazů vhodně zvolte. Měl by také existovat příkaz pro opuštění
tohoto režimu, např. „exit“.</pre>
