namespace N17 {
  export const base: number = 17;
  export function add(x: number): number { return base + x; }
}
function over17(x: number): number;
function over17(x: string): string;
function over17(x: number | string): number | string { return x; }
abstract class A17 { abstract f(x: number): number; }
class B17 extends A17 { override f(x: number): number { return x + 1; } }
enum E17 { A = 5 / 2, B, C = 2 ** 3, D }
const angle17 = <number>(17 + 1);
const check17 = {x: angle17} satisfies {x: number};
console.log(N17.add(1), over17(17), new B17().f(1), E17.B, E17.D, check17.x);
