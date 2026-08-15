interface Item441 { id: number; name: string; }
type Key441 = string | number;
export function make441(id: number, name: string): Item441 {
  const value: Item441 = {id, name};
  return value as Item441;
}
export const item441: Item441 = make441(441, "item-441");
