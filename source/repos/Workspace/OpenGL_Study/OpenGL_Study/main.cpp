import "Basic"

class Human {
  private var age: number = 0;

  public func set_age(age: number) : void {
	this.age = age;
  }

  public func get_age() : number{
	return this.age;
  }
}

func main() : void {

  var human_arr: Human[20];

  for(var i: number=0; i<10; i++){
	human_arr[i] = new Human();
	human_arr[i].set_age(i + 5);
  }

  print(human_arr[4].get_age());
}

