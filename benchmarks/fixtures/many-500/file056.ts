interface Item56 { id: number; name: string; }
type Key56 = string | number;
export function make56(id: number, name: string): Item56 {
  const value: Item56 = {id, name};
  return value as Item56;
}
export const item56: Item56 = make56(56, "item-56");
