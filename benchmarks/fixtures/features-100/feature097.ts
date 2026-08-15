function id97<T>(x: T): T { return x; }
class Base97<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child97 extends Base97<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E97 { A, B = 3, C, Label = "label" }
const x97: number = id97<number>(97);
const c97 = new Child97("n", x97);
console.log(c97.name, E97.A, E97[3]);
