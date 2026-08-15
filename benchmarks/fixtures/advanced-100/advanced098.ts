namespace N98 {
  export const base: number = 98;
  export function add(x: number): number { return base + x; }
}
function over98(x: number): number;
function over98(x: string): string;
function over98(x: number | string): number | string { return x; }
abstract class A98 { abstract f(x: number): number; }
class B98 extends A98 { override f(x: number): number { return x + 1; } }
enum E98 { A = 5 / 2, B, C = 2 ** 3, D }
const angle98 = <number>(98 + 1);
const check98 = {x: angle98} satisfies {x: number};
console.log(N98.add(1), over98(98), new B98().f(1), E98.B, E98.D, check98.x);
