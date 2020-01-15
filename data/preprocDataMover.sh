#!/bin/sh

#Check that extracted data exists
if [ -d "./output" ]
then
	#Create destination folders
	#ACCIDENT
	if [ ! -d "./output/accident" ]
	then
		mkdir ./output/accident
		echo "Create destination folder accident"
	fi
	
	#BOMB
	if [ ! -d "./output/bomb" ]
	then
		mkdir ./output/bomb
		echo "Create destination folder bomb"
	fi
	
	#CAR
	if [ ! -d "./output/car" ]
	then
		mkdir ./output/car
		echo "Create destination folder car"
	fi
	
	#CASUALTY
	if [ ! -d "./output/casualty" ]
	then
		mkdir ./output/casualty
		echo "Create destination folder casualty"
	fi
	
	#ELECTRICITY
	if [ ! -d "./output/electricity" ]
	then
		mkdir ./output/electricity
		echo "Create destination folder electricity"
	fi
	
	#FIRE
	if [ ! -d "./output/fire" ]
	then
		mkdir ./output/fire
		echo "Create destination folder fire"
	fi
	
	#FIRE_BRIGADE
	if [ ! -d "./output/fire_brigade" ]
	then
		mkdir ./output/fire_brigade
		echo "Create destination folder fire_brigade"
	fi
	
	#FLOOD
	if [ ! -d "./output/flood" ]
	then
		mkdir ./output/flood
		echo "Create destination folder flood"
	fi
	
	#GAS
	if [ ! -d "./output/gas" ]
	then
		mkdir ./output/gas
		echo "Create destination folder gas"
	fi
	
	#INJURY
	if [ ! -d "./output/injury" ]
	then
		mkdir ./output/injury
		echo "Create destination folder injury"
	fi
	
	#PARAMEDICS
	if [ ! -d "./output/paramedics" ]
	then
		mkdir ./output/paramedics
		echo "Create destination folder paramedics"
	fi
	
	#PERSON
	if [ ! -d "./output/person" ]
	then
		mkdir ./output/person
		echo "Create destination folder person"
	fi
	
	#POLICE
	if [ ! -d "./output/police" ]
	then
		mkdir ./output/police
		echo "Create destination folder police"
	fi
	
	#ROAD_BLOCK
	if [ ! -d "./output/road_block" ]
	then
		mkdir ./output/road_block
		echo "Create destination folder road_block"
	fi
	
	#Arrange output files inside destination folders
	mv ./output/accident* ./output/accident/ 2> /dev/null
	ls ./output/accident/ -l | wc -l && echo "files moved to /accident"
	
	mv ./output/bomb* ./output/bomb/ 2> /dev/null
	ls ./output/bomb/ -l | wc -l && echo "files moved to /bomb"
	
	mv ./output/car* ./output/car/ 2> /dev/null
	ls ./output/car/ -l | wc -l && echo "files moved to /car"
	
	mv ./output/casualty* ./output/casualty/ 2> /dev/null
	ls ./output/casualty/ -l | wc -l && echo "files moved to /casualty"
	
	mv ./output/electricity* ./output/electricity/ 2> /dev/null
	ls ./output/electricity/ -l | wc -l && echo "files moved to /electricity"
	
	mv ./output/fire\ brigade* ./output/fire_brigade/ 2> /dev/null
	ls ./output/fire_brigade/ -l | wc -l && echo "files moved to /fire_brigade"
	
	mv ./output/fire_0* ./output/fire/ 2> /dev/null
	ls ./output/fire/ -l | wc -l && echo "files moved to /fire"
	
	mv ./output/flood* ./output/flood/ 2> /dev/null
	ls ./output/flood/ -l | wc -l && echo "files moved to /flood"
	
	mv ./output/gas* ./output/gas/ 2> /dev/null
	ls ./output/gas/ -l | wc -l && echo "files moved to /gas"
	
	mv ./output/injury* ./output/injury/ 2> /dev/null
	ls ./output/injury/ -l | wc -l && echo "files moved to /injury"
	
	mv ./output/paramedics* ./output/paramedics/ 2> /dev/null
	ls ./output/paramedics/ -l | wc -l && echo "files moved to /paramedics"
	
	mv ./output/person* ./output/person/ 2> /dev/null
	ls ./output/person/ -l | wc -l && echo "files moved to /person"
	
	mv ./output/police* ./output/police/ 2> /dev/null
	ls ./output/police/ -l | wc -l && echo "files moved to /police"
	
	mv ./output/road* ./output/road_block/ 2> /dev/null
	ls ./output/road_block/ -l | wc -l && echo "files moved to /road_block"
			
else
	echo "Directory output doesn't exist, can't re-order extracted data."
fi


