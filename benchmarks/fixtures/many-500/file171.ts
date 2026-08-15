interface Item171 { id: number; name: string; }
type Key171 = string | number;
export function make171(id: number, name: string): Item171 {
  const value: Item171 = {id, name};
  return value as Item171;
}
export const item171: Item171 = make171(171, "item-171");
