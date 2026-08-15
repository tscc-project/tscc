interface Item402 { id: number; name: string; }
type Key402 = string | number;
export function make402(id: number, name: string): Item402 {
  const value: Item402 = {id, name};
  return value as Item402;
}
export const item402: Item402 = make402(402, "item-402");
