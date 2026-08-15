interface Item280 { id: number; name: string; }
type Key280 = string | number;
export function make280(id: number, name: string): Item280 {
  const value: Item280 = {id, name};
  return value as Item280;
}
export const item280: Item280 = make280(280, "item-280");
