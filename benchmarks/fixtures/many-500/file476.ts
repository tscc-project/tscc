interface Item476 { id: number; name: string; }
type Key476 = string | number;
export function make476(id: number, name: string): Item476 {
  const value: Item476 = {id, name};
  return value as Item476;
}
export const item476: Item476 = make476(476, "item-476");
