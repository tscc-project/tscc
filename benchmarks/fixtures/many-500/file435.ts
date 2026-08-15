interface Item435 { id: number; name: string; }
type Key435 = string | number;
export function make435(id: number, name: string): Item435 {
  const value: Item435 = {id, name};
  return value as Item435;
}
export const item435: Item435 = make435(435, "item-435");
