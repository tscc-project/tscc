function id69<T>(x: T): T { return x; }
class Base69<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child69 extends Base69<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E69 { A, B = 3, C, Label = "label" }
const x69: number = id69<number>(69);
const c69 = new Child69("n", x69);
console.log(c69.name, E69.A, E69[3]);
