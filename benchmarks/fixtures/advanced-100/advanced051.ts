namespace N51 {
  export const base: number = 51;
  export function add(x: number): number { return base + x; }
}
function over51(x: number): number;
function over51(x: string): string;
function over51(x: number | string): number | string { return x; }
abstract class A51 { abstract f(x: number): number; }
class B51 extends A51 { override f(x: number): number { return x + 1; } }
enum E51 { A = 5 / 2, B, C = 2 ** 3, D }
const angle51 = <number>(51 + 1);
const check51 = {x: angle51} satisfies {x: number};
console.log(N51.add(1), over51(51), new B51().f(1), E51.B, E51.D, check51.x);
