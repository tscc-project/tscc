interface Item143 { id: number; name: string; }
type Key143 = string | number;
export function make143(id: number, name: string): Item143 {
  const value: Item143 = {id, name};
  return value as Item143;
}
export const item143: Item143 = make143(143, "item-143");
