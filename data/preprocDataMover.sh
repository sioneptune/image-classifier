#!/bin/sh

#Check that extracted data exists
if [ -d "./output" ]
then
	#Create destination folders

	#Create files_output.txt file
	if [ -f "./output/files_output.txt" ]
	then
		# Delete it before filling it, to clean old versions
		rm ./output/files_output.txt
	fi
	touch ./output/files_output.txt
	echo "Create files_output.txt file"

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
	
	cd ./output
	
	#Arrange output files inside destination folders and cat in files_output.txt
	mv ./accident* ./accident/ 2> /dev/null
	ls ./accident/ -l | wc -l && echo "files moved to /accident"
	ls ./accident/accident*.png >> ./files_output.txt
	
	mv ./bomb* ./bomb/ 2> /dev/null
	ls ./bomb/ -l | wc -l && echo "files moved to /bomb"
	ls ./bomb/bomb*.png >> ./files_output.txt

	mv ./car* ./car/ 2> /dev/null
	ls ./car/ -l | wc -l && echo "files moved to /car"
	ls ./car/car*.png >> ./files_output.txt
	
	mv ./casualty* ./casualty/ 2> /dev/null
	ls ./casualty/ -l | wc -l && echo "files moved to /casualty"
	ls ./casualty/casualty*.png >> ./files_output.txt

	mv ./electricity* ./electricity/ 2> /dev/null
	ls ./electricity/ -l | wc -l && echo "files moved to /electricity"
	ls ./electricity/electricity*.png >> ./files_output.txt	

	mv ./fire\ brigade* ./fire_brigade/ 2> /dev/null
	ls ./fire_brigade/ -l | wc -l && echo "files moved to /fire_brigade"
	ls ./fire_brigade/fire\ brigade*.png >> ./files_output.txt	

	mv ./fire_0* ./fire/ 2> /dev/null
	ls ./fire/ -l | wc -l && echo "files moved to /fire"
	ls ./fire/fire_0*.png >> ./files_output.txt	

	mv ./flood* ./flood/ 2> /dev/null
	ls ./flood/ -l | wc -l && echo "files moved to /flood"
	ls ./flood/flood*.png >> ./files_output.txt
	
	mv ./gas* ./gas/ 2> /dev/null
	ls ./gas/ -l | wc -l && echo "files moved to /gas"
	ls ./gas/gas*.png >> ./files_output.txt
	
	mv ./injury* ./injury/ 2> /dev/null
	ls ./injury/ -l | wc -l && echo "files moved to /injury"
	ls ./injury/injury*.png >> ./files_output.txt

	mv ./paramedics* ./paramedics/ 2> /dev/null
	ls ./paramedics/ -l | wc -l && echo "files moved to /paramedics"
	ls ./paramedics/paramedics*.png >> ./files_output.txt	

	mv ./person* ./person/ 2> /dev/null
	ls ./person/ -l | wc -l && echo "files moved to /person"
	ls ./person/person*.png >> ./files_output.txt	

	mv ./police* ./police/ 2> /dev/null
	ls ./police/ -l | wc -l && echo "files moved to /police"
	ls ./police/police*.png >> ./files_output.txt	
	
	mv ./road* ./road_block/ 2> /dev/null
	ls ./road_block/ -l | wc -l && echo "files moved to /road_block"
	ls ./road_block/road*.png >> ./files_output.txt	
			
else
	echo "Directory output doesn't exist, can't re-order extracted data."
fi


