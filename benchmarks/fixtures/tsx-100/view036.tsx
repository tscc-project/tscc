interface Props36 { value: number; label: string }
export const View36 = (p: Props36) => (
  <section data-i="36">
    <h2>{p.label}</h2>
    {p.value > 36 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
