
function id<T>(value: T): T { return value; }
class Pair<T> {
  constructor(public left: T, readonly right: T) {}
  map<U>(fn: (x: T) => U): Pair<U> { return new Pair<U>(fn(this.left), fn(this.right)); }
}
class Child extends Pair<number> {
  constructor(public name: string, left: number, right: number) {
    super(left, right);
    console.log(name);
  }
}
enum Direction { Up, Down = 3, Left, Label = "label" }

const a = id<string>("x");
const p = new Pair<number>(1, 2);
const c = new Child("kid", 4, 5);
console.log(a, p.left, p.right, c.name, Direction.Up, Direction[3], Direction.Left, Direction.Label);
