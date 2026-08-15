interface Item387 { id: number; name: string; }
type Key387 = string | number;
export function make387(id: number, name: string): Item387 {
  const value: Item387 = {id, name};
  return value as Item387;
}
export const item387: Item387 = make387(387, "item-387");
