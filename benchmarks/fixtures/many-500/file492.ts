interface Item492 { id: number; name: string; }
type Key492 = string | number;
export function make492(id: number, name: string): Item492 {
  const value: Item492 = {id, name};
  return value as Item492;
}
export const item492: Item492 = make492(492, "item-492");
