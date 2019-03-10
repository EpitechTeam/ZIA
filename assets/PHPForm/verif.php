<html lang="en">

<style>
@import url("https://fonts.googleapis.com/css?family=Merriweather:400,400i,700");
body{
	background: #123;
	color: #fff;

}
.parallax-image{
	background-image: url("https://images.unsplash.com/photo-1508329250266-873ca051e559?ixlib=rb-0.3.5&q=85&fm=jpg&crop=entropy&cs=srgb&ixid=eyJhcHBfaWQiOjE0NTg5fQ&s=730c169976af0339132af2bf02150502");
height: 500px;
	background-attachment:fixed;
	background-position: center;
	background-repeat: no-repeat;
	background-size: cover;
}

.parallax-image1{
	background-image: url("https://images.unsplash.com/photo-1523151594509-9d2e49774fec?ixlib=rb-0.3.5&q=85&fm=jpg&crop=entropy&cs=srgb&ixid=eyJhcHBfaWQiOjE0NTg5fQ&s=615391cfee2012537e0e99e5be14999e");
height: 500px;
	background-attachment:fixed;
	background-position: center;
	background-repeat: no-repeat;
	background-size: cover;
}
h1{
	font-family: Merriweather, serif;

}

</style>

  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">

    <title>Hello, world!</title>
  </head>
  <body>
    <!-- Optional JavaScript -->
    <!-- jQuery first, then Popper.js, then Bootstrap JS -->
    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>


    <div class="container">
    	<div class="parallax-image mb-5">
    		<div class="row h-100">
    			<div class="col-md-12 align-self-center">

<?php
$prenom = $_GET['prenom'];
$nom = $_GET['nom'];


$array = array("Abdel", "Ilyes", "Monty", "David", "Leo", "Marvyn");

print("<center><h1>Here is the data you entered:</h1> <h4>Prenom: $prenom</h4>  <h4>Name: $nom</h4><br></center>");

?>

    			</div>
    		</div>

    	<div class="parallax-image1 my-5">
    		<div class="row h-100">
    			<div class="col-md-12 align-self-center">
    				<h1 class="text-center border">Team</h1>
    				<?php
    				    foreach ($array as $i => $value) {
                                print("<center><p>$array[$i]</p></center>");
                        }
                        ;
                        print("<br>");
    				?>
    			</div>
    		</div>
    	</div>
    </div>

  </body>
</html>
