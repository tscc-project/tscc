function id63<T>(x: T): T { return x; }
class Base63<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child63 extends Base63<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E63 { A, B = 3, C, Label = "label" }
const x63: number = id63<number>(63);
const c63 = new Child63("n", x63);
console.log(c63.name, E63.A, E63[3]);
