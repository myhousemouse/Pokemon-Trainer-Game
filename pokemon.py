import random
import time
import json
import tkinter as tk
from tkinter import messagebox

# 포켓몬 클래스
class Pokemon:
    def __init__(self, name, attribute, attack, hp, level=1, exp=0):
        self.name = name
        self.attribute = attribute
        self.attack = attack
        self.hp = hp
        self.max_hp = hp
        self.level = level
        self.exp = exp

    def take_damage(self, damage):
        self.hp -= damage
        if self.hp < 0:
            self.hp = 0

    def heal(self, amount):
        self.hp += amount
        if self.hp > self.max_hp:
            self.hp = self.max_hp

    def gain_exp(self, exp_amount):
        self.exp += exp_amount
        if self.exp >= self.level * 50:
            self.level += 1
            self.exp = 0
            self.attack += 20
            self.max_hp += 50
            self.hp = self.max_hp
            print(f"{self.name}가 레벨업 하였습니다! (Lv.{self.level})")

    def __str__(self):
        return f"{self.name} (Lv.{self.level}) - HP: {self.hp}/{self.max_hp}, ATK: {self.attack}"

# 트레이너 클래스 (사용자)
class Trainer:
    def __init__(self, name):
        self.name = name
        self.pokemons = []
        self.money = 10000
        self.bag = {"몬스터볼": 5, "회복물약": 2}

    def add_pokemon(self, pokemon):
        if len(self.pokemons) < 6:
            self.pokemons.append(pokemon)
            print(f"{pokemon.name}를 포획하였습니다!")
        else:
            print("더 이상 포켓몬을 소지할 수 없습니다!")

    def use_potion(self, pokemon):
        if self.bag["회복물약"] > 0:
            pokemon.heal(50)
            self.bag["회복물약"] -= 1
            print(f"{pokemon.name}의 체력이 회복되었습니다! ({pokemon.hp}/{pokemon.max_hp})")
        else:
            print("회복 물약이 없습니다!")

    def use_pokeball(self, enemy_pokemon):
        if self.bag["몬스터볼"] > 0:
            self.bag["몬스터볼"] -= 1
            catch_rate = 40 if enemy_pokemon.hp < (enemy_pokemon.max_hp * 0.5) else 10
            if random.randint(1, 100) <= catch_rate:
                self.add_pokemon(enemy_pokemon)
            else:
                print("포켓몬이 몬스터볼에서 빠져나왔다!")
        else:
            print("몬스터볼이 없습니다!")

    def show_pokemons(self):
        for idx, pokemon in enumerate(self.pokemons, start=1):
            print(f"{idx}. {pokemon}")

# 게임 관리 클래스
class GameManager:
    def __init__(self):
        self.trainer = None
        self.pokemon_list = [
            Pokemon("파이리", "불", 200, 500),
            Pokemon("이상해씨", "풀", 130, 700),
            Pokemon("꼬부기", "물", 100, 1000),
            Pokemon("치코리타", "풀", 150, 750),
            Pokemon("불꽃숭이", "불", 170, 600)
        ]

    def start_game(self):
        name = input("트레이너의 이름을 입력하세요: ")
        self.trainer = Trainer(name)
        self.choose_pokemon()
        self.main_menu()

    def choose_pokemon(self):
        print("어느 포켓몬을 고르시겠습니까?")
        for idx, pokemon in enumerate(self.pokemon_list[:3], start=1):
            print(f"{idx}. {pokemon.name} - {pokemon.attribute}")
        choice = int(input("선택 번호: ")) - 1
        self.trainer.add_pokemon(self.pokemon_list[choice])

    def battle(self):
        print("야생의 포켓몬이 나타났다!")
        enemy_pokemon = random.choice(self.pokemon_list)
        print(f"적 포켓몬: {enemy_pokemon}")

        player_pokemon = self.trainer.pokemons[0]
        while player_pokemon.hp > 0 and enemy_pokemon.hp > 0:
            print("\n1. 공격  2. 가방  3. 도망치기")
            action = input("선택: ")

            if action == "1":
                damage = player_pokemon.attack * random.uniform(0.8, 1.2)
                enemy_pokemon.take_damage(int(damage))
                print(f"{player_pokemon.name}이(가) {enemy_pokemon.name}에게 {int(damage)}의 피해를 입혔습니다.")

                if enemy_pokemon.hp == 0:
                    print(f"{enemy_pokemon.name}을(를) 쓰러뜨렸다!")
                    player_pokemon.gain_exp(30)
                    return

            elif action == "2":
                print("1. 회복 물약  2. 몬스터볼")
                item = input("사용할 아이템: ")
                if item == "1":
                    self.trainer.use_potion(player_pokemon)
                elif item == "2":
                    self.trainer.use_pokeball(enemy_pokemon)

            elif action == "3":
                print("도망쳤습니다!")
                return

            enemy_damage = enemy_pokemon.attack * random.uniform(0.8, 1.2)
            player_pokemon.take_damage(int(enemy_damage))
            print(f"{enemy_pokemon.name}이(가) {player_pokemon.name}에게 {int(enemy_damage)}의 피해를 입혔습니다.")

    def main_menu(self):
        while True:
            print("\n1. 여행 떠나기  2. 포켓몬 목록  3. 상점 가기  4. 저장하기  5. 종료")
            choice = input("선택: ")

            if choice == "1":
                self.battle()
            elif choice == "2":
                self.trainer.show_pokemons()
            elif choice == "3":
                self.shop()
            elif choice == "4":
                self.save_game()
            elif choice == "5":
                print("게임을 종료합니다.")
                break

    def shop(self):
        print("상점 - 소지금:", self.trainer.money)
        print("1. 몬스터볼 (1000원)  2. 회복 물약 (2500원)")
        choice = input("구매할 아이템: ")
        if choice == "1" and self.trainer.money >= 1000:
            self.trainer.money -= 1000
            self.trainer.bag["몬스터볼"] += 1
            print("몬스터볼을 구매했습니다.")
        elif choice == "2" and self.trainer.money >= 2500:
            self.trainer.money -= 2500
            self.trainer.bag["회복물약"] += 1
            print("회복 물약을 구매했습니다.")
        else:
            print("돈이 부족합니다.")

# 게임 실행
game = GameManager()
game.start_game()
