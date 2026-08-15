function id17<T>(x: T): T { return x; }
class Base17<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child17 extends Base17<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E17 { A, B = 3, C, Label = "label" }
const x17: number = id17<number>(17);
const c17 = new Child17("n", x17);
console.log(c17.name, E17.A, E17[3]);
