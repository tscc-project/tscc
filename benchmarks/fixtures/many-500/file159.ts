interface Item159 { id: number; name: string; }
type Key159 = string | number;
export function make159(id: number, name: string): Item159 {
  const value: Item159 = {id, name};
  return value as Item159;
}
export const item159: Item159 = make159(159, "item-159");
