namespace N24 {
  export const base: number = 24;
  export function add(x: number): number { return base + x; }
}
function over24(x: number): number;
function over24(x: string): string;
function over24(x: number | string): number | string { return x; }
abstract class A24 { abstract f(x: number): number; }
class B24 extends A24 { override f(x: number): number { return x + 1; } }
enum E24 { A = 5 / 2, B, C = 2 ** 3, D }
const angle24 = <number>(24 + 1);
const check24 = {x: angle24} satisfies {x: number};
console.log(N24.add(1), over24(24), new B24().f(1), E24.B, E24.D, check24.x);
