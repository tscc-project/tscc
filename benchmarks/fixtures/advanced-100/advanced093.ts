namespace N93 {
  export const base: number = 93;
  export function add(x: number): number { return base + x; }
}
function over93(x: number): number;
function over93(x: string): string;
function over93(x: number | string): number | string { return x; }
abstract class A93 { abstract f(x: number): number; }
class B93 extends A93 { override f(x: number): number { return x + 1; } }
enum E93 { A = 5 / 2, B, C = 2 ** 3, D }
const angle93 = <number>(93 + 1);
const check93 = {x: angle93} satisfies {x: number};
console.log(N93.add(1), over93(93), new B93().f(1), E93.B, E93.D, check93.x);
