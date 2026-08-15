namespace N21 {
  export const base: number = 21;
  export function add(x: number): number { return base + x; }
}
function over21(x: number): number;
function over21(x: string): string;
function over21(x: number | string): number | string { return x; }
abstract class A21 { abstract f(x: number): number; }
class B21 extends A21 { override f(x: number): number { return x + 1; } }
enum E21 { A = 5 / 2, B, C = 2 ** 3, D }
const angle21 = <number>(21 + 1);
const check21 = {x: angle21} satisfies {x: number};
console.log(N21.add(1), over21(21), new B21().f(1), E21.B, E21.D, check21.x);
