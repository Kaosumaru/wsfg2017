Sterowanie

1 gracz - joypad 1, WSAD VB
2 gracz - joypad 2, strza�ki ,.

joypady trzeba pod��czy� przed w��czeniem gry.

Runy:

czerwona runa - atak, odejmuje 5 HP
zielona runa - leczenie, leczy 3
fioletowa - przeciwnikowi spadaj� kryszta�ki
��ta - spowolnienie kryszta�k�w na 3 sekundy
niebieska - zamra�a kryszta�ki przeciwnika


Ew zmiany:

game/actions/actions.msl mo�na edytowa� ManaCost (ile kryszta�k�w trzeba matchowa�, �eby uruchomi� efekt)

game/players/player.msl 

Health zmienia ile �ycia ma gracz
Game.Action.Speed()[2.5] 2.5 to mno�nik pr�dko�ci kiedy trzymasz drugi przycisk
MinSpeed - pr�dko�� na pocz�tek gry, MaxSpeed pr�dko�� pod koniec gry (mniejsza = szybciej)
GameDuration - czas gry (po tylu sekundach pr�dko�� gry b�dzie wynosi�a MaxSpeed)
