<?php
	$versionActuel = "1.1.0";
	$versionBeta = "1.1.1-dev";

	$versionAutoriser[0] =  $versionBeta;
	$versionAutoriser[1] =  $versionActuel;
//	$versionAutoriser[2] =  "1.0.0";

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{

		$versionTest = $_POST["version"];

		$autoriserConnexion = 0;
		foreach ($versionAutoriser as $value) {
			if( $versionTest == $value )
				$autoriserConnexion = 1;
		}

		echo $autoriserConnexion;
	}



?>
