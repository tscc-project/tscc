interface Item420 { id: number; name: string; }
type Key420 = string | number;
export function make420(id: number, name: string): Item420 {
  const value: Item420 = {id, name};
  return value as Item420;
}
export const item420: Item420 = make420(420, "item-420");
