interface Item371 { id: number; name: string; }
type Key371 = string | number;
export function make371(id: number, name: string): Item371 {
  const value: Item371 = {id, name};
  return value as Item371;
}
export const item371: Item371 = make371(371, "item-371");
