Sterowanie

1 gracz - joypad 1, WSAD VB
2 gracz - joypad 2, strza³ki ,.

joypady trzeba pod³¹czyæ przed w³¹czeniem gry.

Runy:

czerwona runa - atak, odejmuje 5 HP
zielona runa - leczenie, leczy 3
fioletowa - przeciwnikowi spadaj¹ kryszta³ki
¿ó³ta - spowolnienie kryszta³ków na 3 sekundy
niebieska - zamra¿a kryszta³ki przeciwnika


Ew zmiany:

game/actions/actions.msl mo¿na edytowaæ ManaCost (ile kryszta³ków trzeba matchowaæ, ¿eby uruchomiæ efekt)

game/players/player.msl 

Health zmienia ile ¿ycia ma gracz
Game.Action.Speed()[2.5] 2.5 to mno¿nik prêdkoœci kiedy trzymasz drugi przycisk
MinSpeed - prêdkoœæ na pocz¹tek gry, MaxSpeed prêdkoœæ pod koniec gry (mniejsza = szybciej)
GameDuration - czas gry (po tylu sekundach prêdkoœæ gry bêdzie wynosi³a MaxSpeed)
