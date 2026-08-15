interface Item271 { id: number; name: string; }
type Key271 = string | number;
export function make271(id: number, name: string): Item271 {
  const value: Item271 = {id, name};
  return value as Item271;
}
export const item271: Item271 = make271(271, "item-271");
