namespace N76 {
  export const base: number = 76;
  export function add(x: number): number { return base + x; }
}
function over76(x: number): number;
function over76(x: string): string;
function over76(x: number | string): number | string { return x; }
abstract class A76 { abstract f(x: number): number; }
class B76 extends A76 { override f(x: number): number { return x + 1; } }
enum E76 { A = 5 / 2, B, C = 2 ** 3, D }
const angle76 = <number>(76 + 1);
const check76 = {x: angle76} satisfies {x: number};
console.log(N76.add(1), over76(76), new B76().f(1), E76.B, E76.D, check76.x);
