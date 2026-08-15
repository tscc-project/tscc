interface Item250 { id: number; name: string; }
type Key250 = string | number;
export function make250(id: number, name: string): Item250 {
  const value: Item250 = {id, name};
  return value as Item250;
}
export const item250: Item250 = make250(250, "item-250");
